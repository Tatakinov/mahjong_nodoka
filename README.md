# mahjong\_nodoka

## これは何？

麻雀の手牌の向聴数を求めるSAORIです。

## 使い方

### 向聴数を求める

```
Argument0: shanten
Argument1: 手牌
Argument2: 自分から見えている牌(手牌、河、副露、ドラ表示牌)

Result: 向聴数,有効牌,不要牌
```

#### 例

```
Argument0: shanten
Argument1: 3m4m4m5m5m6m6m7m7m8m9m<1m2m3m>
Argument2: 1m2m3m3m4m4m5m5m6m6m7m7m8m9m
Result: 0,6m8m9m7m3m2m5m4m,6m4m3m7m9m
```

なお、牌はソートされている必要はありません。

#### 有効牌の取得方法

そのまま使うと有効牌かつ不要牌な牌が存在します。
各打牌毎の有効牌を取得する時は

1. 14枚で呼んで不要牌を取得

2. 不要牌を1つ取り除く

3. 13枚で呼んで有効牌の取得

のようにすると良いでしょう。

### 現時点で成立している役を調べる

```
Argument0: yaku
Argument1: 手牌(自摸を除く)
Argument2: 自摸
Argument3: 見えている牌
Argument4: ドラ(ドラ表示牌ではない)
Argument5: 場風
Argument6: 自風

Result: 翻,不要牌,成立している役(スラッシュ区切り)
```

向聴数を求めるのと違い、最大の翻になる組み合わせにおける不要牌のみが出力されます。
なお、面前や立直などは手牌によらない役は含まれません。

#### 例

```
Argument0: yaku
Argument1: <1m2m3m>3m4m4m5m5m6m6m7m7m8m
Argument2: 9m
Argument3: 1m2m3m4m4m5m5m6m6m7m7m8m9m
Argument4: 1m
Argument5: 1z
Argument6: 2z

Result: 7,3m7m,Ikki/Chinitsu/Dora
```

