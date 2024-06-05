# Scala

## Как запускать?

### [Пример чения-записи в файл](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November16th/FileInputOutputExample.scala)
Предварительно создать два текстовых файла в папке проекта.

### [Пример задачи линейного программирования](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November23th/Main28.scala)
- Установить библиотеку commons-math3-3.2.jar;
- Добавить в проект: Project Structure -> Modules -> + -> JARs or Directories... -> выбрать файл -> open -> Apply -> OK.

### [Пример работы со статистикой](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November30th/Statistics.scala)
Аналогично предыдущему случаю.

### [Обработка текста на примере моделей en-token.bin, en-ner-person.bin](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November16th/Main25.scala)
- Скачать архив apache-opennlp-2.3.1;
- Распаковать;
- Из apache-opennlp-2.3.1->lib скопировать файл opennlp-tools-2.3.1.jar в папку проекта;
- Добавить в проект: Project Structure -> Modules -> + -> JARs or Directories... -> выбрать файл -> open -> Apply -> OK.
- Скачать файлы en-token.bin и en-ner-person.bin;
- Скопировать в папку проекта.

### [Разбиение текста на токены](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November23th/Main26.scala)
Аналогично предыдущему случаю.

### [Пример с именной группой](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November23th/Main27.scala)
- Первые 4 шага - аналогично предыдущему случаю.
- Дополнительно скачать файл en-parser-chunking.bin;
- Скопировать в папку проекта.

### [Веб-программирование на примере Play Framework](https://github.com/SneakyElfff/Scala/tree/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November30th)
- Для новых версий Play Framework: следовать инструкции по настройке и созданию проекта на официальном сайте;
- Для запуска первого проекта (MacOS):
    - открыть терминал;
    - sbt;
    - run;
- Зайти в папку проекта. Далее app->views;
- Изменить содержимое файла index.scala.html на содержимое [index.html](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November30th/index.html);
- Вернуться в папку проекта. Далее app->controllers;
- Добавить свой [контроллер](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/November30th/MyHomeController.scala);
- Вернуться в папку проекта. Далее conf->controllers;
- Изменить содержимое файла routes на:

\# Routes<br>
\# This file defines all application routes (Higher priority routes first)<br>
\# https://www.playframework.com/documentation/latest/ScalaRouting<br>
\# ~~~~

\# Маршрут для главной страницы (index.html)<br>
GET     /                           controllers.MyHomeController.index()

\# Маршрут для динамического действия MyHomeController.hello()<br>
GET     /hello                      controllers.MyHomeController.hello()

\# Map static resources from the /public folder to the /assets URL path<br>
GET     /assets/*file               controllers.Assets.versioned(path="/public", file: Asset)

- В браузере открыть http://localhost:9000.

### [Пример для чтения массива пикселей](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/December7th/Main29.scala)
Предварительно добавить картинку .jpg в папку проекта.

### [Пример для определения статистических параметров картинки](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/December7th/Main30.scala)
- Аналогично предыдущему случаю;
- Аналогично [случаю](#пример-задачи-линейного-программирования).

### [Пример для сравнения двух картинок](https://github.com/SneakyElfff/Scala/blob/84bfb613a2cecda231d1282d7ae0d865724fd3e5/Lectures/December7th/Main31.scala)
- Предварительно добавить две картинки .jpg в папку проекта;
- Аналогично [случаю](#пример-задачи-линейного-программирования).
