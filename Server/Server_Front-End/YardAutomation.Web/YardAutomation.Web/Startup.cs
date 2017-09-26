using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(YardAutomation.Web.Startup))]
namespace YardAutomation.Web
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}
