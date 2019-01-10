# otter
  C Language Word Cutting Tool Based on Shortest Path

# Install

<pre>
python2:
 sudo pip install --upgrade https://github.com/xuenhappy/otter/raw/master/otter-0.10-cp27-cp27mu-linux_x86_64.whl

---------------------------------------------------------------
 python3:

 sudo pip install --upgarde https://github.com/xuenhappy/otter/raw/master/otter-0.10-cp36-cp36m-linux_x86_64.whl
</pre>

# Useage:
<pre>
import otter
str=u"中国驻加大使：加方要求放人时，何曾考虑过孟晚舟! this is testmax."
print("|".join(otter.cut(str)))
</pre>