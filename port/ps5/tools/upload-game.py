#!/usr/bin/env python3
"""Xerenge PS5: copy an extracted disc (default.xex, data/, tracks/, ...) to the
console over FTP (ftpsrv, port 2121), where the game finds it:
/data/xerenge/game by default.

    python3 -I port/ps5/tools/upload-game.py --addr <console> game

Each file goes up as NAME.part and is renamed once its read-back SHA-256
matches; a file already there with the same size and SHA-256 is kept, so an
interrupted copy goes on where it stopped. Remote files the folder doesn't have
are listed, not deleted."""
import argparse
import ftplib
import hashlib
import pathlib
import socket
import time


def remote_sha256(ftp, path):
    hashed = hashlib.sha256()
    ftp.retrbinary('RETR ' + path, hashed.update, blocksize=1 << 20)
    return hashed.hexdigest()


def local_sha256(path):
    with path.open('rb') as source:
        return hashlib.file_digest(source, 'sha256').hexdigest()


def listing(ftp, path, base=''):
    sizes, lines = {}, []
    try:
        ftp.retrlines('LIST ' + path, lines.append)
    except ftplib.error_perm:
        return sizes
    for line in lines:
        fields = line.split(None, 8)
        if len(fields) < 9 or fields[8] in ('.', '..'):
            continue
        name = base + fields[8]
        if line.startswith('d'):
            sizes.update(listing(ftp, path + '/' + fields[8], name + '/'))
        else:
            sizes[name] = int(fields[4])
    return sizes


def remove(ftp, path):
    try:
        ftp.delete(path)
    except ftplib.error_reply as error:
        # ftpsrv uses 226 for a successful DELE; ftplib expects 250.
        if not str(error).startswith('226'):
            raise
    except ftplib.error_perm as error:
        if not str(error).startswith('550'):
            raise


def connect(addr):
    ftp = ftplib.FTP()
    ftp.connect(addr, 2121, timeout=300)
    ftp.login()
    ftp.voidcmd('TYPE I')
    return ftp


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--addr', required=True)
    p.add_argument('--dest', default='/data/xerenge/game')
    p.add_argument('game', type=pathlib.Path)
    a = p.parse_args()
    if not (a.game / 'default.xex').is_file():
        raise SystemExit('no default.xex in ' + str(a.game))
    names = sorted(f.relative_to(a.game).as_posix() for f in a.game.rglob('*')
                   if f.is_file() and f.name != '.DS_Store')
    total = sum((a.game / n).stat().st_size for n in names)
    ftp = connect(a.addr)
    remote = listing(ftp, a.dest)
    for name in sorted(set(remote) - set(names)):
        print('only on console:', name)
    made, done, sent, kept, began = set(), 0, 0, 0, time.time()
    for index, name in enumerate(names, 1):
        local = a.game / name
        size = local.stat().st_size
        target = a.dest + '/' + name
        digest = local_sha256(local)
        for attempt in range(4):
            try:
                if remote.get(name) == size and remote_sha256(ftp, target) == digest:
                    kept += 1
                    break
                parent = ''
                for part in target.strip('/').split('/')[:-1]:
                    parent += '/' + part
                    if parent not in made:
                        try:
                            ftp.mkd(parent)
                        except ftplib.error_perm:
                            pass
                        made.add(parent)
                with local.open('rb') as source:
                    ftp.storbinary('STOR ' + target + '.part', source, blocksize=1 << 20)
                if remote_sha256(ftp, target + '.part') != digest:
                    raise SystemExit('readback failed, left as ' + target + '.part')
                remove(ftp, target)
                ftp.rename(target + '.part', target)
                sent += 1
                break
            except (OSError, EOFError, ftplib.error_temp, socket.timeout) as error:
                print('retrying', name, repr(error), flush=True)
                try:
                    ftp.close()
                except OSError:
                    pass
                time.sleep(3 * (attempt + 1))
                ftp = connect(a.addr)
                remote = listing(ftp, a.dest)
        else:
            raise SystemExit('gave up on ' + name)
        done += size
        if index % 20 == 0 or index == len(names):
            rate = done / max(time.time() - began, 1) / (1 << 20)
            print(f'{index}/{len(names)} files, {done >> 20}/{total >> 20} MiB, '
                  f'{rate:.0f} MiB/s, sent {sent}, kept {kept}', flush=True)
    ftp.quit()
    print('done: every file matches', flush=True)


if __name__ == '__main__':
    main()
