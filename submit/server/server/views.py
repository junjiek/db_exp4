from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render_to_response

import db, time

__all__ = [
    'main',
    'search',
]

init = False

def main(request):
    global init
    if not init:
        db.read_json('zipcode-address.json')
        init = True
    return render_to_response('main.html')


def search(request):
    t0 = time.clock()
    pois = db.search(int(request.GET['num']), float(request.GET['xmin']),
                     float(request.GET['ymin']), float(request.GET['xmax']),
                     float(request.GET['ymax']))
    t1 = time.clock()
    return HttpResponse(str(t1-t0) + ';' + str(pois))
