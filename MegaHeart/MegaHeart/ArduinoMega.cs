using MegaHeart;
using System;

namespace MegaHeart
{
    public class ArduinoMega
    {
        public ArduinoMega()
        {
            timestamp = this.getEpoch();
        }
        public ArduinoMega(float temperature, float humidity, float lux)
        {

            this.timestamp = this.getEpoch();
            this.humidity = humidity;
            this.lux = lux;
            this.temperature = temperature;
        }

        private long getEpoch()
        {
            DateTime unixEpoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
            DateTime utcNow = DateTime.UtcNow;
            TimeSpan elapsedTime = utcNow - unixEpoch;
            return Convert.ToInt64(elapsedTime.TotalMilliseconds);
        }
        public float temperature { get; set; }

        public float humidity;

        public float lux { get; set; }
        //public float soil { get; set; }
        public long timestamp { get; }

    }
}
public class ArduinoMegaJSON : ArduinoMega
{
    public ArduinoMegaJSON(int relay, int roride, ArduinoMega Mega) : base (Mega.temperature, Mega.humidity, Mega.lux)
    {
        this.relay = relay;
        this.roride = roride;
    }
    public ArduinoMegaJSON(int relay, int roride, float temperature, float humidity, float lux) : base(temperature, humidity, lux)
    {
        this.relay = relay;
        this.roride = roride;
    }
    public int relay { get; set; }
    public int roride { get; set; }
}
