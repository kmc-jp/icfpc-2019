# benchmaker

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

TODO: 計算量で殴るための並列化

