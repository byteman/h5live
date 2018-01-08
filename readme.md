# h5live的设计目标

* 一个轻量级可嵌入式的C++流媒体服务器
* 一个支持各种浏览器的实时视频流播放器
* 服务器以库的形式提供
* 主要用于视频预览，对长时间稳定连接无特别需求
* 局域网内延迟时间控制在500ms以内
* 跨平台支持(linux,windows,android,ios).


# h5live不支持什么
* h5live不支持视频采集，仅支持YUV或者RGB输入，由流媒体内部编码(针对没有自己编码器的应用)，或者直接输入编码后的h264码流(针对有自己编码器的应用)

# h5live技术方案
* 1.后台服务器提供一个websocket服务器，将h264视频mux为flv流，流推送给H5播放器，H5播放器(flv.js)可以直接播放flv流
* 2.后台服务器提供一个websocket服务器，将h264视频流推送给H5播放器，H5播放器(flv.js)封装264为mp4，进行播放，需要修改flv.js源码
* 3.先采用方案1.

# 整体架构

 ![h5整体架构](https://github.com/byteman/h5live/raw/master/doc/h5整体架构.png )
 ![h5live内部实现](https://github.com/byteman/h5live/raw/master/doc/h5live内部架构.png )

# 服务端编译说明

为了实现跨平台的编译服务端代码，工程采用qmake来构建

## windows

* 需要安装qt5.5+vs2012

## linux

## android


