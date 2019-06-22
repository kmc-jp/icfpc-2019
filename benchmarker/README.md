# benchmaker

- `REDME.md` : これ
- `bench.rb` : ベンチファイル (Ruby)
- `problems/` : 問題たち
- `results/` : `./bench run`の結果が出る一時ディレクトリ。良さがあったらコミットしよう
- `best/` : `./bench verify`で良い結果が出たらここにコピーされる
- `verifier/` : nodeの実行するやつ
  + `npm i`を予めする
  + `npm run verify PROB OUTPUT`
  + `npm run verify` だとインタラクティブモード
- `bestzip/` : zipされるやつ `./bench genzip`
- `bestzip.zip` : 提出すべきファイル `./bench genzip`