# ブロックを生み出したら負け
第12回 Siv3D Game Jam
テーマ（ブロック）

## 概要
作者：Rinifisu (https://twitter.com/rinifisu)

制作においての縛り
- ブロックで実用可能なテーマを１つ加えること（切断）
- 制作は６時間以内で済ませる
- Main.cpp だけに収める
- 外部ファイルは一切使用せずに、Example で済ませる
- エフェクトクラスを１つ以上作る

## 内容  
中心を狙って「ブロックの生産を最小限に抑える」謎ゲームです（笑）
キーボードでカッターの位置を決定します。

カッターがレンガの上で移動しているので、「２つのカッターが重なるタイミング」でキーボードを押します。
すると、カッターが下へ移動し、レンガを切断します。
幅が狭いほどブロックの大きさを抑えることができます。
切断したブロックは画面下にストックされ、画面サイズを上回る大きさになるとゲームオーバーです。

Window::Resize(★, 480);
★の部分のみ変更可能です。画面幅が広がります。

![スクリーンショット](https://github.com/Rinifisu/Siv3D-Game-Jam/12/Screenshot/1.png "スクリーンショット")

![スクリーンショット](https://github.com/Rinifisu/Siv3D-Game-Jam/12/Screenshot/2.png "スクリーンショット")

![スクリーンショット](https://github.com/Rinifisu/Siv3D-Game-Jam/12/Screenshot/3.png "スクリーンショット")

![スクリーンショット](https://github.com/Rinifisu/Siv3D-Game-Jam/12/Screenshot/4.png "スクリーンショット")