# mahjong\_nodoka

## これは何？

麻雀の手牌の向聴数を求めるSAORIです。

## 使い方

### 向聴数を求める

```
Argument0: shanten
Argument1: 鳴いた手牌を除いた手牌
Argument2: 鳴いた面子の数
Argument3: 自分から見えている牌(手牌、河、副露、ドラ表示牌)

Result: 向聴数,有効牌,捨て牌
```

#### 例

```
Argument0: shanten
Argument1: 5m6m8m8m9m9m1p3p5p7p8p4s4s5s
Argument2: 0
Argument3: 5m6m8m8m9m9m1p3p5p7p8p4s4s5s

Result: 3,5p5s3p1p8p6m9m7m9p6p4m2p4s4p6s3s8m5m7p,9m6m8p4s1p5p3p5s8m7p5m
```

#### 有効牌の取得方法

そのまま使うと有効牌かつ不要牌な牌が存在します。
各打牌毎の有効牌を取得する時は

1. 14枚で呼んで不要牌を取得

2. 不要牌を1つ取り除く

3. 13枚で呼んで有効牌の取得

のようにすると良いでしょう。

