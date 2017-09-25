using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Web;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;
using YardAutomation.Web.Enums;

namespace YardAutomation.Web.Helpers
{
    public class MqttHelper
    {
        private readonly MqttClient _client;
        private string Topic { get; set; }

        private MqttHelper()
        {
        }

        public MqttHelper(string ipAddress = "", string topic = "")
        {
            _client = ipAddress == "" ? new MqttClient(IPAddress.Parse("127.0.0.1")) : new MqttClient(IPAddress.Parse(ipAddress));
            byte code = _client.Connect(Guid.NewGuid().ToString());
            Topic = topic;
        }

        public void SendCommand(string topic, string command)
        {
            if (_client != null)
            {
                ushort msgId = _client.Publish(topic, // topic
                    Encoding.UTF8.GetBytes(command), // message body
                    MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, // QoS level
                    false);
            }
            SendCommand("");
        }

        public void SendCommand(string command)
        {
            if (_client != null)
            {
                ushort msgId = _client.Publish(Topic, // topic
                    Encoding.UTF8.GetBytes(command), // message body
                    MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, // QoS level
                    false);
            }
        }

    }
}