package additional

import scala.collection.mutable.ListBuffer

object Main33 {
  def makeFromTwoListsOne(l1: List[Int], l2: List[Int], l: ListBuffer[Int]): ListBuffer[Int] = {
    if (!l1.isEmpty && !l2.isEmpty) {
      if (l1.head < l2.head) {
        l += l1.head
        makeFromTwoListsOne(l1.tail, l2, l)
      }
      else {
        l += l2.head
        makeFromTwoListsOne(l1, l2.tail, l)
      }
    }

    else {
      l ++= l1
      l ++= l2

      l.tail
    }
  }

  def countElements(list: ListBuffer[Int]): Int = {
    if(list.isEmpty) 0
    else 1 + countElements(list.tail)
  }

  def main(args: Array[String]): Unit = {
    val list1 = List(1, 3, 5, 7, 9)
    val list2 = List(2, 4, 6, 8, 10)
    val list = ListBuffer(0)

    val l = makeFromTwoListsOne(list1, list2, list)
    println("Final list: " + l)
    println("Number of elements: " + countElements(l))
  }
}
