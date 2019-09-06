# otter
  C Language Word Cutting Tool Based on Shortest Path

# Install

<pre>
本程序依赖:

ubuntu执行: apt install libenchant-dev
centos执行: yum install enchant-devel
macos 执行: brew install enchant
-------------------------------------------------------------

macos:
请使用otter-mac-0.10.tar.gz

python2:
 sudo pip2 install --upgrade https://github.com/xuenhappy/otter/raw/master/otter-0.10.tar.gz

---------------------------------------------------------------
 python3:

 sudo pip3 install --upgrade https://github.com/xuenhappy/otter/raw/master/otter-0.10.tar.gz
</pre>

# Useage:
<pre>
import otter
str=u"中国驻加大使：加方要求放人时，何曾考虑过孟晚舟! this is testmax."
print("|".join(otter.cut(str)))
</pre>