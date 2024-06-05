package November23th

/*
18. Аналитические вычисления в Scala. Пример задачи линейного программирования.
 */

import org.apache.commons.math3.optim.PointValuePair
import org.apache.commons.math3.optim.linear.{LinearConstraint, LinearConstraintSet, LinearObjectiveFunction, Relationship, SimplexSolver}
import org.apache.commons.math3.optim.nonlinear.scalar.GoalType

import java.util

object Main28 {
  def main(args: Array[String]): Unit = {
    val coeffs: Array[Double] = Array(2.0, 3.0)
    val coeffs2: Array[Double] = Array(1.0, 3.0)
    val coeffs3: Array[Double] = Array(1.0, 2.0)

    val relationship: Relationship = Relationship.LEQ    //<=

    val value: Double = 4.0

    val coeffs1: Array[Double] = Array(-1.0, 3.0)

    val relationship2: Relationship = Relationship.GEQ    //>=
    val relationship3: Relationship = Relationship.GEQ

    val value2: Double = 1.0
    val value3: Double = 4.0

    val objectiveFunction = new LinearObjectiveFunction(coeffs2, 0.0)    //целевая функция, коэффициенты при перем.

    val constraint: LinearConstraint = new LinearConstraint(coeffs, relationship, value)    //огр.
    val constraint2: LinearConstraint = new LinearConstraint(coeffs1, relationship2, value2)
    val constraint3: LinearConstraint = new LinearConstraint(coeffs3, relationship3, value3)

    val solver = new SimplexSolver()

    val constraintsList2: java.util.List[LinearConstraint] = new util.ArrayList[LinearConstraint]()
    constraintsList2.add(constraint)
    constraintsList2.add(constraint2)
    constraintsList2.add(constraint3)

    val result: PointValuePair = solver.optimize(objectiveFunction, new LinearConstraintSet(constraintsList2), GoalType.MINIMIZE)    //вызов задачи оптимизации; аргументы: целев. функц., огр., тип ф.

    println(s"Minimum value: ${result.getValue}")    //значение целевой функции в точке оптимума
    println(s"Solution: ${result.getPoint.mkString(", ")}")    //значение целевой функции и переменной в точке оптимума
    println("good")
  }
  }