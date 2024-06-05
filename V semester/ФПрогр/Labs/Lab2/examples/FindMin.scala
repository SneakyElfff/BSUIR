package examples

import scala.io.StdIn

object Main7 {

  def minList(lst: List[Int], minval: Int): Int = {
    if (lst.isEmpty) minval    //не работает
    else if (lst.head < minval) minList(lst.tail, lst.head)
    else minList(lst.tail, minval)
  }

  def main(args: Array[String]): Unit = {
    println("Enter elements of the list (integers), separated by spaces: ")
    val myList = StdIn.readLine().split(" ").map(_.toInt).toList

    println("Entered list: " + myList)

    if (myList.isEmpty) {
      println("The list is empty")
    }
    else {
      val minw = minList(myList, myList.head)
      println(minw)
    }
  }
}