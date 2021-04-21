
* 本ソースコードは慶應義塾大学と三菱UFJインフォメーションテクノロジー株式会社の共同研究として作成したものです。
* 研究成果を社会に向けて広く公開することで、分散データベース研究の今後の発展へ貢献することを目指しています。
* MUFG及びグループ企業で現行稼働するシステムと関連するものではありません。


## Building

* Raftノード
```
cd src/server
make
```

* Clientアプリケーション
```
cd src/client
make
```

## Raftノードの実行方法

```
./src/server/raft ${raft config file}
```

* raft config file: ノードの接続先情報 e.g. ./config/raft.conf


## Clientアプリケーションの実行方法
```	
./src/client/client ${Input file}  ${client config file} ${TX_NUM}
```

* Input file: 送金元ID 送金先ID 送金額をスペース区切りで1行ずつ記載したもの。e.g. ./INPUT/input_20k
* client config file: Raftノードへの接続先情報 e.g. ./config/client.conf
* TX_NUM: 1クライアントプロセスあたりの送付するトランザクション数。整数で指定。
