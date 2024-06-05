package var1

object Main11 {
  def findSumOfChosen(list: List[Int], cond: Int): Int = {
    if (list.isEmpty) 0
    else {
      if (list.head < cond) list.head + findSumOfChosen(list.tail, cond)
      else findSumOfChosen(list.tail, cond)
    }
  }

  def findSumOfFirstThree(list: List[Int], num: Int): Int = {
    if (list.isEmpty) 0
    else {
      if (list.length > num - 3) list.head + findSumOfFirstThree(list.tail, num)
      else 0
    }
  }

  def findFirstMax(list: List[Int], max: Int, ind_current: Int, ind_max: Int): Int = {
    if (list.isEmpty) ind_max
    else {
      if (list.head > max) {
        findFirstMax(list.tail, list.head, ind_current + 1, ind_current)
      }
      else {
        findFirstMax(list.tail, max, ind_current + 1, ind_max)
      }
    }
  }

  def isOrderedByIncreasing(list: List[Int]): Boolean = {
    if (list.isEmpty || list.length == 1) true
    else {
      if (list(1) > list.head) isOrderedByIncreasing(list.tail)
      else false
    }
  }

//  def checkForRepeatedValues(list: List[Int]): Boolean = {
//    for (i <- 0 until list.length) {
//      for (j <- i+1 until list.length) {
//        if (list(i) == list(j)) return true
//      }
//    }
//
//    false
//  }

  def checkForRepeatedValues(list: List[Int]): Boolean = {    //more functional method
    list.groupBy(identity).values.exists(_.length > 1)
  }

  def main(args: Array[String]): Unit = {
    val list = List(86, 39, 3, 14, 5, 23, 95, 39, 95)
    val list2 = List(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
    println(list)

    val sum_particular = findSumOfChosen(list, 10)
    println("\nThe sum of elements less than 10: " + sum_particular)

    val sum_of_three = findSumOfFirstThree(list, list.length)
    println("\nThe sum of the first 3 elements: " + sum_of_three)

    val ind_of_max = findFirstMax(list, list.head, 0, 0)
    println("\nThe index of the max element: " + ind_of_max)

    val is_ordered = isOrderedByIncreasing(list2)
    if (is_ordered) println("\nThe list is ordered by increasing");
    else println("\nThe list isn't ordered")

    val repeated_values = checkForRepeatedValues(list)
    if (repeated_values) println("\nThere are repeated elements")
    else println("\nThere aren't any repeated elements")
  }
}