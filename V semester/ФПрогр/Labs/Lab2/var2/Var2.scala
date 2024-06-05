package var2

object Main22 {
  def findSumOfNegatives(list: List[Int]): Int = {
    if (list.isEmpty) 0
    else {
      if (list.head < 0) list.head + findSumOfNegatives(list.tail)
      else findSumOfNegatives(list.tail)
    }
  }

  def findSumOfLastThree(list: List[Int]): Int = {
    if (list.isEmpty) 0
    else {
      if (list.length <= 3) list.head + findSumOfLastThree(list.tail)
      else findSumOfLastThree(list.tail)
    }
  }

  def findAllMaxes(list: List[Int], max: Int, init: List[Int]): List[Int] = {
    if (list.isEmpty) {
      init.zipWithIndex.collect {
        case (element, index) if element == max => index
      }
    }
    else if (list.head > max) findAllMaxes(list.tail, list.head, init)
    else findAllMaxes(list.tail, max, init)
  }

  def isOrderedByIncreasing(list: List[Int]): Boolean = {
    if (list.isEmpty || list.length == 1) true
    else {
      if (list(1) > list.head) isOrderedByIncreasing(list.tail)
      else false
    }
  }

  def isOrderedByDecreasing(list: List[Int]): Boolean = {
    if (list.isEmpty || list.length == 1) true
    else {
      if (list(1) < list.head) isOrderedByDecreasing(list.tail)
      else false
    }
  }

  def isOrdered(list: List[Int]): Int = {
    if (isOrderedByIncreasing(list)) 1
    else if (isOrderedByDecreasing(list)) -1
    else 0
  }

//    def findThreeCopies(list: List[Int]): Int = {
//      var value = 0
//      var counter = 0
//
//      for (i <- 0 until list.length) {
//        for (j <- i+1 until list.length) {
//          if (list(i) == list(j)) {
//            value = list(i)
//            counter += 1
//
//            if(counter == 3) return value
//          }
//        }
//      }
//
//      -1
//    }

  def findThreeCopies(list: List[Int]): Int = { //more functional method
    val groups_of_values = list.groupBy(identity)
    val copy = groups_of_values.find { case (_, values) => values.length == 3 }

    copy match {
      case Some((value, _)) => value
      case None => -1
    }
  }

  def main(args: Array[String]): Unit = {
    val list = List(3, -5, 4, 10, -12, 3, -25, 19, 3, 84)
    val list2 = List(86, 39, 3, 14, 5, 23, 95, 39, 95)

    val sum_of_negatives = findSumOfNegatives(list)
    println("\nThe sum of the negative elements: " + sum_of_negatives)

    val sum_of_three = findSumOfLastThree(list)
    println("\nThe sum of the last 3 elements: " + sum_of_three)

    val indeces_max = findAllMaxes(list2, list.head, list2)
    println("\nThe indeces of all the max elements: " + indeces_max.mkString(", "))

    val order = isOrdered(list) match {
      case 1 => "\nThe list is ordered by increasing"
      case -1 => "\nThe list is ordered by decreasing"
      case 0 => "\nThe list isn't ordered"
      case _ => "\nUnknown result"
    }
    println(order);

    val copy = findThreeCopies(list)
    if(copy != -1) println("\nThe element, repeated three times: " + copy)
    else println("\nThere aren't needed copies")
  }
}
