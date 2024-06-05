package examples

object Main10 {
  def main(args: Array[String]): Unit = {
    val str =
      """
        |Hello,
        |World!
        |""".stripMargin

    print(str)    //"\nHello,\nWorld!\n"
  }
}