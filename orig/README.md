# orig/

Put your own copy of the game here. Nothing in this directory except this
file and the `.sha1` lists is tracked.

| Target | File | SHA-1 |
| --- | --- | --- |
| Trial 0.13 (archive) | `kouma_tr013.lzh` | `dfeb4dd358c6613ec4d8b701d41672ca75b6a0c9` |
| Trial 0.13 | `東方紅魔郷.exe` | `066356936f92716bb8ddf6615a1353d4c97b2e8a` |
| Trial 0.13 | `custom.exe` | `0e6b130604696db6048220df946f67cbce9a285e` |
| Full 1.02h | `東方紅魔郷.exe` | TBD |

The trial is distributed for free by Team Shanghai Alice:
<https://www16.big.or.jp/~zun/html/th06.html>

```sh
curl -fLO https://www16.big.or.jp/~zun/data/soft/kouma_tr013.lzh
7z x -otrial013 kouma_tr013.lzh   # filenames inside are Shift-JIS
sha1sum -c trial013.sha1
```

The full game needs a retail CD, updated with the official
`kouma_update102h.EXE` patch from the same page.
