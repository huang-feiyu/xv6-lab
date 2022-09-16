# Chapter 5

> Interrupts and device drivers

The decoupling of device activity from process activity via buffering and
interrupts, can increase performance by allowing processes to execute
concurrently with device I/O.

Interrupts make sense when a device needs attention at unpredictable times, and
not too often.

---

MIT 6.S081 Lecture 9:
* 中断对应的场景很简单，就是硬件想要得到操作系统的关注
* 因为我们主要关心的是外部设备的中断，而不是定时器中断或者软件中断
* PLIC: RISC-V Platform-Level Interrupt Controller
* PLIC需要保存一些内部数据来跟踪中断的状态
    * PLIC会通知当前有一个待处理的中断
    * 其中一个CPU核会Claim接收中断，这样PLIC就不会把中断发给其他的CPU处理
    * CPU核处理完中断之后，CPU会通知PLIC
    * PLIC将不再保存中断的信息
* 大部分驱动都分为两个部分，bottom/top:
    * bottom部分通常是Interrupt handler。当一个中断送到了CPU，并且CPU设置接收这个中断,
      CPU会调用相应的Interrupt handler。Interrupt handler并不运行在任何特定进程的context中，它只是处理中断。
    * top部分，是用户进程，或者内核的其他部分调用的接口。对于UART来说，这里有read/write接口,
      这些接口可以被更高层级的代码调用。
* 当网卡收到大量包，并且处理器不能处理这么多中断的时候该怎么办呢？
  这里的解决方法就是使用polling。除了依赖Interrupt，CPU可以一直读取外设的控制寄存器,
  来检查是否有数据。对于UART来说，我们可以一直读取RHR寄存器，来检查是否有数据。现在，CPU不停的在轮询设备，直到设备有了数据。

> I cannot read this, it's boring...

