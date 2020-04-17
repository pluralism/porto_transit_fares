## porto_transit_fares
Cost optimizer for Porto public transit network.

Side project that calculates the best cost at the end of the month in Porto public transit network. The lib accepts as input a set of trips and returns the set of tickets that match the best combination for the given set of trips. Each trip must have a set of stops, with the respective zone and timestamp.

The best combination of tickets is calculated by using a variation of the **Rod Cutting problem** (occasional tickets) + **Steiner tree** (passes).

**Dijkstra's algorithm with min heap** was also implemented to calculate the distance between two given zones.

Developed in QT C++.
