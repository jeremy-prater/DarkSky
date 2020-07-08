from astropy.coordinates import EarthLocation, SkyCoord, AltAz
import astropy.coordinates as coord
from astropy.time import Time


# Generate some coordinates with a time stamp of utcnow

lat = 45.962034
lon = -122.411777
alt = 360

timestamp = Time.now()

location = EarthLocation.from_geodetic(
    lon=lon, lat=lat, height=alt)

print(location)

sun = coord.get_sun(timestamp)

sun_angle = sun.transform_to(AltAz(obstime=timestamp, location=location))

print(sun_angle)

