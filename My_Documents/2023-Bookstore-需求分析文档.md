## 2023-Bookstore-需求分析文档

### 0. 引言
本文档旨在描述该系统的需求和功能，  ~~参照高中信息技术课程的《信息系统开发的一般过程》编写~~。

### 1. 目标期待与功能需求
#### 1.1核心需求
**要求实现一个面向顾客、销售人员和店长的书店管理系统。**
##### 1.1.1 游客功能（Guest, User access=0,Guest Functionality）
- Account -> Register
- Account -> Login
##### 1.1.2 顾客功能（User, User access=1,Basic Functionality）
- Bookinfo -> Info：每种图书包含ISBN号、书名、作者、关键字、库存量和单价。
- Bookinfo -> Query：顾客可以根据ISBN号、作者名字、关键字、书名进行查询。
- Selling -> Purchase：顾客可以通过输入ISBN号和购买数量来购买图书。
##### 1.1.3 销售人员功能（Advanced User, User access=2, Advanced Functionality）
销售人员可以执行顾客的所有操作，并具有额外的功能。
-  Selling -> Stocking：进货操作，需指定ISBN号、数量和进货价格
- Bookinfo -> Addition：信息录入
- Bookinfo -> Modification：信息修改
- Account -> Register
##### 1.1.4 店长功能（Administer, User access=3, Administer Functionality）
店长额外的功能：
- Sellng -> Query(Purchase_Information)：店长可以查询某段时间的采购信息，按照采购顺序输出图书的ISBN号、数量和单价等。
- Selling -> Query(Sales Situation)：店长可以查询某段时间的销售情况，按照销售顺序输出图书的ISBN号、数量和单价等。
- Selling -> Query(Profit Information)：店长可以查询某段时间的盈利信息，包括收入、支出和利润。
- Document -> Employee Work Report：店长可以查看各员工的工作情况报告，按操作顺序输出每次操作类型及其对象。
- Document -> System Work Log：店长可以查看系统的整体工作日志。

##### 1.1.5 其他需求
- 语言：C++
- 存储方式：外存
- 初次运行：初始化并创建根账户（System, User access=4)
- 交互：命令行
#### 1.2拓展需求&创新需求
- 一些bonus需求（如界面设计）
- 可以实现：csv导入导出

#### 1.3性能需求，资源和环境需求，用户界面需求，可拓展性需求
略略略

### 2.系统及其子系统设计
#### 2.1子系统
**子系统**：Account，Bookinfo, Selling, Document
对于Account子系统：
  用户信息
  注册，登录，用户信息修改
  （用户栈）和（当前用户）
对于Bookinfo子系统：
  书籍信息
  查询，修改，增加，删除
对于Selling子系统：
  购买（出售）
  进货
  查看库存
  查询（店长）
  （生成Document？）

调度主系统


某个操作通用流程：？

身份认证（Identity）：用户名和密码（或安全密钥文件）

访问控制（Access Management）
分别是账号（Account）、认证（Authentication）和授权（Authorization）
审计（Audit）