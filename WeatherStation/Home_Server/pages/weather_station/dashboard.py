from flask import render_template

from db.db import db_get_weather_data_one_measurement, data_update_period


def dashboard_page():
    period = (60 * 24) / data_update_period
    # current data
    temperature_in = db_get_weather_data_one_measurement('temperature', '1', '0')
    temperature_out = db_get_weather_data_one_measurement('temperature', '1', '1')
    humidity_in = db_get_weather_data_one_measurement('humidity', '1', '0')
    humidity_out = db_get_weather_data_one_measurement('humidity', '1', '1')
    pressure = db_get_weather_data_one_measurement('pressure', '1')
    dew_point = db_get_weather_data_one_measurement('dew_point', '1')
    # previous data
    prev_temperature_in = db_get_weather_data_one_measurement('temperature', '2', '0')
    prev_temperature_out = db_get_weather_data_one_measurement('temperature', '2', '1')
    prev_humidity_in = db_get_weather_data_one_measurement('humidity', '2', '0')
    prev_humidity_out = db_get_weather_data_one_measurement('humidity', '2', '1')
    prev_pressure = db_get_weather_data_one_measurement('pressure', '2')
    prev_dew_point = db_get_weather_data_one_measurement('dew_point', '2')
    # last day data
    last_day_temperature_in = db_get_weather_data_one_measurement('temperature', f'{period}', '0')
    last_day_temperature_out = db_get_weather_data_one_measurement('temperature', f'{period}', '1')
    last_day_humidity_in = db_get_weather_data_one_measurement('humidity', f'{period}', '0')
    last_day_humidity_out = db_get_weather_data_one_measurement('humidity', f'{period}', '1')
    last_day_pressure = db_get_weather_data_one_measurement('pressure', f'{period}')
    last_day_dew_point = db_get_weather_data_one_measurement('dew_point', f'{period}')
    return render_template("dashboard.html",
                           temperature_in=temperature_in,
                           temperature_out=temperature_out,
                           humidity_in=humidity_in,
                           humidity_out=humidity_out,
                           pressure=pressure,
                           dew_point=dew_point,
                           prev_temperature_in=prev_temperature_in,
                           prev_temperature_out=prev_temperature_out,
                           prev_humidity_in=prev_humidity_in,
                           prev_humidity_out=prev_humidity_out,
                           prev_pressure=prev_pressure,
                           prev_dew_point=prev_dew_point,
                           last_day_temperature_in=last_day_temperature_in,
                           last_day_temperature_out=last_day_temperature_out,
                           last_day_humidity_in=last_day_humidity_in,
                           last_day_humidity_out=last_day_humidity_out,
                           last_day_pressure=last_day_pressure,
                           last_day_dew_point=last_day_dew_point,
                           )
