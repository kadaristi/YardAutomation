    using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
    using YardAutomation.Web.Enums;
    using YardAutomation.Web.Helpers;

namespace YardAutomation.Web.Controllers
{
    public class AjaxController : Controller
    {

        public ActionResult SendCommand(bool state, string message, string topic, string ipAddress)
        {
            MqttHelper mqttHelper = new MqttHelper(ipAddress,topic);
            mqttHelper.SendCommand(state?Commands.Start1:Commands.Stop1);
            return Json("chamara", JsonRequestBehavior.AllowGet);
        }


    }
}