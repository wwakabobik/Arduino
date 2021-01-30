from plotly import graph_objects, offline

from db.weather_station import data_update_period, get_last_series_measurement


units = {'temperature': '°C', 'humidity': '%', 'pressure': 'mm Hg', 'dew_point': '°C'}


def convert_period(period):
    if period == "hour":
        num_period = int(60);
    elif period == "day":
        num_period = int(60 * 24)
    elif period == "week":
        num_period = int(60 * 24 * 7)
    elif period == "month":
        num_period = int(60 * 24 * 30)
    elif period == "year":
        num_period = int(60 * 24 * 365)
    else:
        raise Exception('Wrong period', f'{period}')
    return num_period


def convert_param(param):
    if "_in" in param:
        meas_type = 0
        fparam = param[:-3]
    elif "_out" in param:
        meas_type = 1
        fparam = param[:-4]
    else:
        meas_type=0
        fparam=param
    return fparam, meas_type


def generate_scatter(param, period, height=None, width=None):
    fparam, meas_type = convert_param(param)
    num_period = convert_period(period)
    x, y = get_last_series_measurement(period=num_period, param=fparam, meas_type=meas_type)
    line_chart1 = graph_objects.Scatter(x=x, y=y)
    lay1 = graph_objects.Layout(
        title=f'{param}',
        yaxis=dict(title=f'{units[fparam]}', ticks=f'inside'),
        height=height,
        width=width
    )
    fig1 = graph_objects.Figure(data=line_chart1, layout=lay1)
    offline_fig = offline.plot(fig1,
                               config={"displayModeBar": False},
                               show_link=False,
                               include_plotlyjs=True,
                               output_type='div')
    return offline_fig
