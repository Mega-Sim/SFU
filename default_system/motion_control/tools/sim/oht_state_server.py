#!/usr/bin/env python3
# -*- coding:utf-8 -*-
from flask import Flask, render_template, json, jsonify, request, redirect
from oht_signal_logger import *


app = Flask(__name__, static_url_path='/uidata/static', static_folder='uidata/static', template_folder='uidata/templates')
groups = {}
status = None

def initialize():
    global groups
    for group in OHTSignalLogger.groups:
        groups[group] = {'di': [], 'do': []}
        last_di_index = OHTSignalLogger.groups[group].index(OHTSignalLogger.last_di_flags[group])
        for i, flag in enumerate(OHTSignalLogger.groups[group]):
            if i <= last_di_index:
                groups[group]['di'].append(flag)
            else:
                groups[group]['do'].append(flag)


@app.route('/')
def main_page():
    global groups
    return render_template("oht_states.html", groups=groups)


@app.route('/update')
def update_state():
    global status
    status = OHTSignalLogger.get_status()    
    return jsonify(status['oht']['dio'])


@app.route('/update_loc')
def update_location():
    ''' call just after update_state() '''
    global status
    return jsonify({
        'location_from': status['oht']['location']['from'],
        'location_to':status['oht']['location']['to'],
        'location_displacement': '%.3f' % status['oht']['location']['disp'],
        'hoist_pos':status['oht']['position']['hoist'],
        'slide_pos':status['oht']['position']['slide'],
    })


@app.route('/get_speed_array')
def get_speed_array():
    return jsonify({
        'time_array': OHTSignalLogger.time_log,
        'speed_array': OHTSignalLogger.speed_log
    })


@app.route('/update_speed')
def update_speed():
    return jsonify({
        'time': OHTSignalLogger.time_log[-1],
        'speed': '%.1f' % OHTSignalLogger.speed_log[-1]
    })


if __name__ == "__main__":
    initialize()
    app.run(host="0.0.0.0", port=9001)
