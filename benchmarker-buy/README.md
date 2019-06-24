# benchmaker

## setup

```
bundle install --path=vendor/bundle
```

## usage

```
bundle exec ruby bench.rb run {prog.o} : プログラムを実行してディレクトリに吐く
bundle exec ruby bench.rb verify {dir} : ディレクトリの検査をしてbestをアップデートをする
bundle exec ruby bench.rb genzip : bestをzipに固めて bestzip.zipを作る
bundle exec ruby bench.rb check {prog.o} {prob-XXX.desc} : 単一実行して検査 bestは更新しない (verifyすれば更新される)
```

- `REDME.md` : これ
- `bench.rb` : ベンチファイル (Ruby)
- `problems/` : 問題たち
- `results/` : `./bench run`の結果が出る一時ディレクトリ。良さがあったらコミットしよう
- `best/` : `./bench verify`で良い結果が出たらここにコピーされる
- `verifier/` : nodeの実行するやつ。実行時間が求まる。
  + `npm i`を予めする
  + `npm run verify PROB OUTPUT`
  + `npm run verify` だとインタラクティブモード
  + クソ遅いから検査無しで求めj時間使うほうが良いかも…
- `bestzip/` : zipされるやつ `./bench genzip`
- `bestzip.zip` : 提出すべきファイル `./bench genzip`

TODO: 相対パスだとverifyとかが動かない… 適当に書きすぎた…
TODO: verifier重すぎる。実行時間を求めるのに使うのは現実的ではない。

