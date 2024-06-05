package controllers

/*
23. Веб-программирование на примере Play Framework.
 */

import javax.inject._
import play.api.mvc._

@Singleton
class MyHomeController @Inject()(val controllerComponents: ControllerComponents) extends BaseController {
  def index() = Action { implicit request: Request[AnyContent] =>
    Ok(views.html.index())  // Имя файла шаблона может отличаться в зависимости от вашей структуры проекта
  }

  def hello() = Action { implicit request: Request[AnyContent] =>
    Ok("Hello, world! Hi You")
  }
}

//для запуска play-приложения: sbt, new, в браузере localhost:9000