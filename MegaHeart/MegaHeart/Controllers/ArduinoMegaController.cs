using System;
using System.IO;
using System.Net;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace MegaHeart.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class ArduinoMegaController : ControllerBase
    {

        private readonly ILogger<ArduinoMegaController> _logger;
        string path = @"jsondata.json";


        public ArduinoMegaController(ILogger<ArduinoMegaController> logger)
        {
            _logger = logger;
        }

        [HttpGet]
        public ActionResult<dynamic> Get()
        {
            _logger.LogInformation("GET triggered.");

            return getLastReadings();
        }

        [HttpPost]
        public ActionResult<dynamic> Post([FromForm]float humidity, [FromForm]float temperature, [FromForm]float lux, [FromForm]float relay, [FromForm]float roride)
        {
            _logger.LogInformation("POST triggered.");

            ArduinoMega relayInputModel = new ArduinoMega(temperature, humidity, lux);
            ArduinoMegaJSON relayInputModelJSON = new ArduinoMegaJSON(Convert.ToInt32(relay), Convert.ToInt32(roride), relayInputModel);


            try
            {
                //convert object to json string.
                string json = JsonConvert.SerializeObject(relayInputModelJSON);

                //export data to json file. 
                using (TextWriter tw = new StreamWriter(this.path))
                {
                    tw.WriteLine(json);
                };
                //return new StatusCodeResult(200);

                _logger.LogInformation("Data stored.");

            }
            catch (Exception ex)
            {
                _logger.LogError("Something unexpectedly broke while trying to fetch arduino data.");
                _logger.LogError(ex.Message.ToString());
                return new StatusCodeResult(502);

            }
            return new StatusCodeResult(200);

        }
        private dynamic getLastReadings()
        {

            dynamic retVal = null;
            try
            {

                using (StreamReader r = new StreamReader(this.path))
                {
                    retVal = r.ReadToEnd();
                }
            }
            catch (WebException ex)
            {
                _logger.LogError("Error occured while getting last values");
                _logger.LogError(ex.Message.ToString());
            }

            return retVal;
        }

    }
}
