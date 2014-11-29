    我自己使用的Duilib和Uilib库中，修复了我在博客中说明的所有bug以及很多还没有去说明的bug，同样也包含别人修复过的bug。修复的部分bug可以看"Duilib和UIlib更新日志.txt"，完整的修复过程描述请看我博客。

    库里面包含我扩展过的部分控件。修复了编译duilib时出现的““IDispatchEx”: 重定义；不同的基类型”和'ICanHandleException' : 重定义错误。

    另外库里添加了“Duilib扩展群”群主heat的透明异形窗体功能，RichEdit支持在透明窗体中使用，可以显示光标。Edit控件我增加了trans属性，开启后可以支持透明窗体 

    带有文本功能的控件增加了autocalcwidth属性，可以让控件自动根据文字长度计算宽度。附带了基于wke内核的webkit浏览器控件的源码和demo。库中修复了relativepos属性，可以让绝对布局用起来更方便，属性列表文件中补全了一部分属性，但是最完整的属性列表请自己看源码SetAttribute函数·····

    这个库是我用vs2010修改的。如果使用时至少需要vs2010版本。使用高版本打开时，如果提示升级工程，点“是”，然后询问是否覆盖原来的工程配置信息，一定记住要选“否”。
     
    工程支持Release、Debug、UnicodeRelease、UnicodeDebug四种配置方案。我只配置了Unicode方案，需要多字节方案的朋友请仿照Unicode方案自己配置。其他情况如果你无法正常打开我的库或者编译有问题，那么建议把你需要的.h或者.cpp文件替换掉你自己的duilib库对应的文件来使用。

    库中我修改过的代码，一般可以通过搜索“redrain”找到对应的位置和修改时间

    此库是我个人使用，由于我水平所限，所以不保证我修改的代码的质量。请你看过代码后再决定是否使用。使用这个库出现问题，不承担任何责任.


   

by:Redrain  2014.11.4    QQ:491646717

blog:http://blog.csdn.net/zhuhongshu?viewmode=contents