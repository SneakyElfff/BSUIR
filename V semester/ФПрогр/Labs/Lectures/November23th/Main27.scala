package November23th

/*
22. Scala-приложение для обработки текста. Пример с именной группой.
 */

import opennlp.tools.cmdline.parser.ParserTool
import opennlp.tools.parser.{Parse, ParserFactory, ParserModel}

import java.io.FileInputStream

object Main27 {
  def getNounPhrases(parse: Parse): List[String] = {    //принимаем объект Parse и рекурсивно извлекаем из него все именные словосочетания
    val nounPhrases = parse.getChildren.filter(_.getType == "NP").map(_.getCoveredText).toList
    val childrenNounPhrases = parse.getChildren.flatMap(getNounPhrases).toList

    nounPhrases ++ childrenNounPhrases
  }    //сначала он фильтрует дочерние элементы разбора и выбирает только те, у которых тип равен NP; результат - коллекция строк - сущностей

  def main(args: Array[String]): Unit = {
    var parserModel = new ParserModel(new FileInputStream("en-parser-chunking.bin"))
    val parser = ParserFactory.create(parserModel)
    val sentence = "The quick brown fox jumps over the lazy dog."
    val topParses = ParserTool.parseLine(sentence, parser, 1)    //содержит результаты разбора
    for (parse <- topParses) {
      val nounPhrases = getNounPhrases(parse)
      println(nounPhrases.mkString(", "))
    }
  }
  }