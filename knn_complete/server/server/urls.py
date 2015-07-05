from django.conf.urls import patterns, include, url
from views import main, search
from django.conf import settings
from django.conf.urls.static import static

urlpatterns = patterns('',
    (r'^$', main),
    (r'^search/$', search),
) + static(settings.STATIC_URL, document_root=settings.STATICFILES_DIRS)