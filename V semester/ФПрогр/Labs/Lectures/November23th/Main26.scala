package November23th

/*
21. Scala-приложение для обработки текста. Разбиение текста на токены.
 */

import opennlp.tools.namefind.{NameFinderME, TokenNameFinderModel}
import opennlp.tools.tokenize.TokenizerME
import java.io.FileInputStream

object Main26 {
  def main(args: Array[String]): Unit = {
    val tokenizerModelIn = new FileInputStream("en-token.bin") //азгрузить модель токенизатора
    val tokenizerModel = new opennlp.tools.tokenize.TokenizerModel(tokenizerModelIn) //обученная модель, которая содержит информацию о правилах для разделения текста на токены
    val tokenizer = new TokenizerME(tokenizerModel) //TokenizerME - класс, предоставляющий методы для токенизации текста с использованием загруженной модели
    val nerModelIn = new FileInputStream("en-ner-person.bin")
    val nerModel = new TokenNameFinderModel(nerModelIn) //содержит информацию об именованных сущеностях
    val ner = new NameFinderME(nerModel) //NameFinderME - класс, предоставляющий методы для выполнения распознавания именованных сущностей в тексте

    val text = "John Smith is a software engineer at Google."
    val tokens = tokenizer.tokenize(text)
    tokens.foreach(println)
  }
}