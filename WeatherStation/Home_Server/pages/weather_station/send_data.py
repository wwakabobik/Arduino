import requests

from db.weather_station import get_last_measurement_pack
from secure_data import wu_station_id, wu_station_pwd


def celsius_to_farenheit(celsius):
    farenheit = 9.0 / 5.0 * float(celsius) + 32
    return farenheit


def mmhg_to_baromin(mmhg):
    return float(mmhg)/25.4


def send_data_to_wu():
    current_data = get_last_measurement_pack('0', '1')
    wu_url = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?"
    wu_creds = "ID=" + wu_station_id + "&PASSWORD=" + wu_station_pwd
    date_str = "&dateutc=now"
    action_str = "&action=updateraw"
    r = requests.get(
        wu_url +
        wu_creds +
        date_str +
        "&humidity=" + "{0:.2f}".format(current_data['humidity']) +
        "&tempf=" + str(celsius_to_farenheit(current_data['temperature'])) +
        "&baromin=" + str(mmhg_to_baromin(current_data['pressure'])) +
        "&dewptf=" + str(celsius_to_farenheit(current_data['dew_point'])) +
        action_str)
    return r.content
