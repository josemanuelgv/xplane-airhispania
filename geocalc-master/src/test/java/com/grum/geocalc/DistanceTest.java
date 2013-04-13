/*
 * Copyright (c) 2012 Romain Gallet
 *
 * This file is part of Geocalc.
 *
 * Geocalc is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * Geocalc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Geocalc. If not, see http://www.gnu.org/licenses/.
 */

package com.grum.geocalc;

import org.apache.log4j.Logger;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * @author rgallet
 */
public class DistanceTest {

    final Logger logger = Logger.getLogger(getClass());

    @Test
    public void testDistance() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        //Richmond
        lat = new DegreeCoordinate(51.4613418);
        lng = new DegreeCoordinate(-0.3035466);
        Point richmond = new Point(lat, lng);

        assertEquals(2700.3261395525724, EarthCalc.getDistance(richmond, kew), 10E-3);
    }

    @Test
    public void testSymetricDistance() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        //Richmond
        lat = new DegreeCoordinate(51.4613418);
        lng = new DegreeCoordinate(-0.3035466);
        Point richmond = new Point(lat, lng);

        assertEquals(EarthCalc.getDistance(richmond, kew), EarthCalc.getDistance(kew, richmond), 10E-10);
    }

    @Test
    public void testZeroDistance() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        assertEquals(EarthCalc.getDistance(kew, kew), 0, 0);
    }

    @Test
    public void testBoundingAreaDistance() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        BoundingArea area = EarthCalc.getBoundingArea(kew, 3000);

        double northEastDistance = EarthCalc.getDistance(kew, area.getNorthEast());
        logger.info("North East => " + northEastDistance);
        assertEquals(3000d, northEastDistance, 1E-3);

        double southWestDistance = EarthCalc.getDistance(kew, area.getSouthWest());
        logger.info("South West => " + southWestDistance);
        assertEquals(3000d, southWestDistance, 1E-3);

        Point northWest = area.getNorthWest();
        double northWestDistance = EarthCalc.getDistance(kew, northWest);
        logger.info("North West => " + northWestDistance);
        assertEquals(3000d, northWestDistance, 2);

        Point southEast = area.getSouthEast();
        double southEastDistance = EarthCalc.getDistance(kew, southEast);
        logger.info("South East => " + southEastDistance);
        assertEquals(3000d, southEastDistance, 2);

        Point middleNorth = new Point(new DegreeCoordinate(area.getNorthEast().getLatitude()),
                new DegreeCoordinate((area.getSouthWest().getLongitude() + area.getNorthEast().getLongitude()) / 2));
        double middleNorthDistance = EarthCalc.getDistance(kew, middleNorth);
        logger.info("Middle North => " + middleNorthDistance);
        assertEquals(2120d, middleNorthDistance, 1);

        Point middleSouth = new Point(new DegreeCoordinate(area.getSouthWest().getLatitude()),
                new DegreeCoordinate((area.getSouthWest().getLongitude() + area.getNorthEast().getLongitude()) / 2));
        double middleSouthDistance = EarthCalc.getDistance(kew, middleSouth);
        logger.info("Middle South => " + middleSouthDistance);
        assertEquals(2120d, middleSouthDistance, 2);

        Point middleWest = new Point(new DegreeCoordinate((area.getNorthEast().getLatitude() + area.getSouthWest().getLatitude()) / 2),
                new DegreeCoordinate(area.getNorthEast().getLongitude()));
        double middleWestDistance = EarthCalc.getDistance(kew, middleWest);
        logger.info("Middle West => " + middleWestDistance);
        assertEquals(2120d, middleWestDistance, 3);

        Point middleEast = new Point(new DegreeCoordinate((area.getNorthEast().getLatitude() + area.getSouthWest().getLatitude()) / 2),
                new DegreeCoordinate(area.getSouthWest().getLongitude()));
        double middleEastDistance = EarthCalc.getDistance(kew, middleEast);
        logger.info("Middle East => " + middleEastDistance);
        assertEquals(2120d, middleEastDistance, 1);
    }

    @Test
    public void testBoundingAreaNorthPole() {
        //North Pole
        Coordinate lat = new DegreeCoordinate(90d);
        Coordinate lng = new DegreeCoordinate(0);
        Point northPole = new Point(lat, lng);

        BoundingArea area = EarthCalc.getBoundingArea(northPole, 10000);
        logger.info("North East => " + area.getNorthEast());
        logger.info("South West => " + area.getSouthWest());
    }

    @Test
    public void testPointRadialDistanceZero() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        Point sameKew = EarthCalc.pointRadialDistance(kew, 45, 0);
        assertEquals(lat.getValue(), sameKew.getLatitude(), 1E-10);
        assertEquals(lng.getValue(), sameKew.getLongitude(), 1E-10);

        sameKew = EarthCalc.pointRadialDistance(kew, 90, 0);
        assertEquals(lat.getValue(), sameKew.getLatitude(), 1E-10);
        assertEquals(lng.getValue(), sameKew.getLongitude(), 1E-10);

        sameKew = EarthCalc.pointRadialDistance(kew, 180, 0);
        assertEquals(lat.getValue(), sameKew.getLatitude(), 1E-10);
        assertEquals(lng.getValue(), sameKew.getLongitude(), 1E-10);
    }

    @Test
    public void testBearing() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        for (int i = 0; i <= 360; i = i + 1) {
            Point test = EarthCalc.pointRadialDistance(kew, i, 10000);
            double bearing = EarthCalc.getBearing(kew, test);
            //modulo 360 to care for when i == 360, which rightly returns a bearing of 0
            assertEquals((i % 360), bearing, 1E-10);
        }
    }

    @Test
    public void testPointRadialDistance() {
        //Kew
        Coordinate lat = new DegreeCoordinate(51.4843774);
        Coordinate lng = new DegreeCoordinate(-0.2912044);
        Point kew = new Point(lat, lng);

        //Richmond
        lat = new DegreeCoordinate(51.4613418);
        lng = new DegreeCoordinate(-0.3035466);
        Point richmond = new Point(lat, lng);

        double distance = EarthCalc.getDistance(kew, richmond);
        double bearing = EarthCalc.getBearing(kew, richmond);

        Point allegedRichmond = EarthCalc.pointRadialDistance(kew, bearing, distance);

        assertEquals(richmond.getLatitude(), allegedRichmond.getLatitude(), 10E-5);
        assertEquals(richmond.getLongitude(), allegedRichmond.getLongitude(), 10E-5);

    }
}
