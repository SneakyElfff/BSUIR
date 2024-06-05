package examples

object Main6 {
  def main(args: Array[String]): Unit = {
    val myList = List(1, -2, 3, -7, -5)

    val countNegative = myList.count(item => item < 0)
    println(countNegative)
  }
}