## 2023-Bookstore-需求分析文档

### 0. 引言
本文档旨在描述该系统的需求和功能，  ~~参照高中信息技术课程的《信息系统开发的一般过程》编写~~。

### 1. 目标期待与功能需求
#### 1.1核心需求
**要求实现一个面向顾客、销售人员和店长的书店管理系统。**
##### 1.1.1 游客功能（Guest, User access=0,Guest Functionality）
- Account -> Register
- Account -> Login
- Account -> Logout (退出当前账户)
##### 1.1.2 顾客功能（User, User access=1,Basic Functionality）
- Bookinfo -> Info：查询每种图书包含ISBN号、书名、作者、关键字、库存量和单价。
- Bookinfo -> Query：顾客可以根据ISBN号、作者名字、关键字、书名进行查询。
- Order -> Purchase：顾客可以通过输入ISBN号和购买数量来购买图书。
##### 1.1.3 销售人员功能（Advanced User, User access=2, Advanced Functionality）
销售人员可以执行顾客的所有操作，并具有额外的功能。
- Order -> Stock：进货操作，需指定ISBN号、数量和进货价格
- Bookinfo -> Add：信息录入
- Bookinfo -> Modify：信息修改
- Bookinfo -> Delete：信息删除
- Account -> Register
- Document -> Record：记录工作内容（以生成员工工作报告）
- Account -> Login：登入客户账户并**创建用户操作（订单）**
##### 1.1.4 店长功能（Administer, User access=3, Administer Functionality）
店长额外的功能：
- Order -> Query(Stock Information)：店长可以查询某段时间的**采购信息**，按照采购顺序输出图书的ISBN号、数量和单价等。
- Order -> Query(Sales Situation)：店长可以查询某段时间的**销售情况**，按照销售顺序输出图书的ISBN号、数量和单价等。
- Order -> Query(Profit Information)：店长可以查询某段时间的**盈利信息**，包括收入、支出和利润。
- Document -> View(Employee_Work_Report)：店长可以查看各**员工的工作情况报告**，按操作顺序输出每次操作类型及其对象。
- Document -> View(System_Work_Log)：店长可以查看系统的**整体工作日志**。

##### 1.1.6 其他需求
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
**子系统**包含：Account，Bookinfo, Order, Document

对于Account子系统，实现：
1.用户信息存储
2.注册，登录，用户信息修改
3.维护（用户栈）和（当前用户）

对于Bookinfo子系统，实现：
1.书籍信息存储
2.书籍信息的查询，修改，增加，删除

对于Order子系统，实现： 
1.购买（出售） 
2.进货
3.查看库存
4.记录操作并提供查询（店长）

对于Document子系统，实现：
1.生成员工工作报告（按操作顺序输出每次操作类型及其对象）
2.工作情况报告查询
3.系统的整体工作日志
（此部分功能将根据标准需求文档修改）

#### 2.2调度主系统
初始化，处理异常，接受输入并调用相应子系统