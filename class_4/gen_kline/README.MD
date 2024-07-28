# generate kline

设计满足了基本要求，并尽量地运行更快。

## build

在linux平台，可运行以下程序构建。

```
bash build.sh
```

## run

第一个参数为csv文件的路径。

```
./generate_kline md_20221110.csv
```

程序会输出四个csv文件,对应不同周期K线。
```
kline_10min.csv
kline_1min.csv
kline_5min.csv
kline_30min.csv
```