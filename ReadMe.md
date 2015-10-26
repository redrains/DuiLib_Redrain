# Redrain DuiLib
我自己使用的Duilib和Uilib库中，修复了我在博客中说明的所有bug以及很多还没有去说明的bug，同样也包含别人修复过的bug。修复的部分bug可以看"Duilib和UIlib更新日志.txt"，完整的修复过程描述请看我博客。

- 库里面包含我扩展过的部分控件。修复了编译duilib时出现的““IDispatchEx”: 重定义；不同的基类型”和'ICanHandleException' : 重定义错误。

- 修改duilib和uilib支持半透明异形窗体，RichEdit支持在透明窗体中使用，可以显示光标。Edit控件可以支持透明窗体。增加gif控件。增加窗体阴影类，可以设置图片阴影或者算法阴影。增加菜单通用类。

- 带有文本功能的控件增加了autocalcwidth属性，可以让控件自动根据文字长度计算宽度。库中修复了relativepos属性，可以让绝对布局用起来更方便。更多属性的增强和添加，详见各个博客和提交代码的提交说明。

- 附带了基于wke内核的webkit浏览器控件的源码和demo、半透明异形窗体demo、Redrain仿酷狗播放器源码、Redrain菜单demo。

- 我附带了vs2008和vs2010两个版本的工程解决方案。使用更高版本的vs打开时，如果提示升级工程，点“是”，然后询问是否覆盖原来的工程配置信息，一定记住要选“否”！  

- 工程支持UnicodeRelease、UnicodeDebug两种配置方案。其他情况如果你无法正常打开我的库或者编译有问题，那么建议把你需要的.h或者.cpp文件替换掉你自己的duilib库对应的文件来使用。

库中我修改过的代码，一般可以通过搜索“redrain”找到对应的位置和修改时间

此库是我个人使用，由于我水平所限，所以不保证我修改的代码的质量。请你看过代码后再决定是否使用。使用这个库出现问题，不承担任何责任.

**by:Redrain  2014.11.4QQ:491646717**

blog:[http://blog.csdn.net/zhuhongshu?viewmode=contents](http://blog.csdn.net/zhuhongshu?viewmode=contents)

# DuiLib和UiLib更新日志
[Duilib和UIlib更新日志](./Duilib和UIlib更新日志.txt)