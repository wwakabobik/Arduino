from flask import render_template
from wunderground_pws import WUndergroundAPI, units

from db.weather_station import get_one_measurement, data_update_period

api_key = 'd2b79805ff4c4717b79805ff4c1717d5'
cur_location = 'Domodedovo (Domodedovo), RU'
wu = WUndergroundAPI(
    api_key=api_key,
    default_station_id='IDOMOD6',
    units=units.METRIC_SI_UNITS,
)


def deg_to_heading(degrees=0):
    dirs = ['N', 'NE', 'E', 'SE', 'S', 'SW', 'W', 'NW']
    ix = round(degrees / (360. / len(dirs)))
    return dirs[ix % len(dirs)]


def dashboard_page():
    # current data
    temperature_in = get_one_measurement('temperature', '0', '0')
    temperature_in = get_one_measurement(param='temperature', offset='0', meas_type='0')
    temperature_out = get_one_measurement('temperature', '0', '1')
    humidity_in = get_one_measurement('humidity', '0', '0')
    humidity_out = get_one_measurement('humidity', '0', '1')
    pressure = get_one_measurement('pressure', '0', '0')
    dew_point_in = get_one_measurement('dew_point', '0', '0')
    dew_point_out = get_one_measurement('dew_point', '0', '1')
    # previous data
    prev_temperature_in = get_one_measurement('temperature', '1', '0')
    prev_temperature_out = get_one_measurement('temperature', '1', '1')
    prev_humidity_in = get_one_measurement('humidity', '1', '0')
    prev_humidity_out = get_one_measurement('humidity', '1', '1')
    prev_pressure = get_one_measurement('pressure', '1', '0')
    prev_dew_point_in = get_one_measurement('dew_point', '1', '0')
    prev_dew_point_out = get_one_measurement('dew_point', '1', '1')
    # last day data
    period = (float(60 * 24) / data_update_period) - 1
    last_day_temperature_in = get_one_measurement('temperature', period, '0')
    last_day_temperature_out = get_one_measurement('temperature', period, '1')
    last_day_humidity_in = get_one_measurement('humidity', period, '0')
    last_day_humidity_out = get_one_measurement('humidity', period, '1')
    last_day_pressure = get_one_measurement('pressure', period, '0')
    last_day_dew_point_in = get_one_measurement('dew_point', period, '0')
    last_day_dew_point_out = get_one_measurement('dew_point', period, '1')
    # last year data
    period = (float(60 * 24 * 365) / data_update_period) - 1
    last_year_temperature_in = get_one_measurement('temperature', period, '0')
    last_year_temperature_out = get_one_measurement('temperature', period, '1')
    last_year_humidity_in = get_one_measurement('humidity', period, '0')
    last_year_humidity_out = get_one_measurement('humidity', period, '1')
    last_year_pressure = get_one_measurement('pressure', period, '0')
    last_year_dew_point_in = get_one_measurement('dew_point', period, '0')
    last_year_dew_point_out = get_one_measurement('dew_point', period, '1')
    # WU forecast data
    wu_temp = wu.current()['observations'][0]['metric_si']['temp']
    wu_humidity = wu.current()['observations'][0]['humidity']
    wu_pressure = int(int(wu.current()['observations'][0]['metric_si']['pressure'])/1.33)
    wu_dew_point = wu.current()['observations'][0]['metric_si']['dewpt']
    wu_wind_speed = wu.current()['observations'][0]['metric_si']['windSpeed']
    wu_wind_gust = wu.current()['observations'][0]['metric_si']['windGust']
    wu_direction = wu.current()['observations'][0]['winddir']
    wu_heading = deg_to_heading(int(wu_direction))

    return render_template("weather_station/dashboard.html",
                           temperature_in=temperature_in,
                           temperature_out=temperature_out,
                           humidity_in=humidity_in,
                           humidity_out=humidity_out,
                           pressure=pressure,
                           dew_point_in=dew_point_in,
                           dew_point_out=dew_point_out,
                           prev_temperature_in=prev_temperature_in,
                           prev_temperature_out=prev_temperature_out,
                           prev_humidity_in=prev_humidity_in,
                           prev_humidity_out=prev_humidity_out,
                           prev_pressure=prev_pressure,
                           prev_dew_point_in=prev_dew_point_in,
                           prev_dew_point_out=prev_dew_point_out,
                           last_day_temperature_in=last_day_temperature_in,
                           last_day_temperature_out=last_day_temperature_out,
                           last_day_humidity_in=last_day_humidity_in,
                           last_day_humidity_out=last_day_humidity_out,
                           last_day_pressure=last_day_pressure,
                           last_day_dew_point_in=last_day_dew_point_in,
                           last_day_dew_point_out=last_day_dew_point_out,
                           last_year_temperature_in=last_year_temperature_in,
                           last_year_temperature_out=last_year_temperature_out,
                           last_year_humidity_in=last_year_humidity_in,
                           last_year_humidity_out=last_year_humidity_out,
                           last_year_pressure=last_year_pressure,
                           last_year_dew_point_in=last_year_dew_point_in,
                           last_year_dew_point_out=last_year_dew_point_out,
                           wu_temp=wu_temp,
                           wu_humidity=wu_humidity,
                           wu_pressure=wu_pressure,
                           wu_dew_point=wu_dew_point,
                           wu_wind_speed=wu_wind_speed,
                           wu_wind_gust=wu_wind_gust,
                           wu_wind_direction=wu_direction,
                           wu_wind_heading=wu_heading
                           )
