package examples

object Main4 {
  def main(args: Array[String]): Unit = {
    val str = "Hello, World!"
    val subStr = str.substring(7, 12)    //[7, 12)

    println(subStr)
  }
}