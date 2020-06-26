ObjectBox TS (Time Series) Demo
===============================
**ObjectBox TS** offers additional time series functionality over the standard ObjectBox database.
This allows very efficient processing of time based data, while still offering object based persistence.

This demo shows how to develop with ObjectBox TS, which is very similar to standard ObjectBox.

**In order to run this demo, you need to get the [ObjectBox TS library from objectbox.io](https://objectbox.io/time-series-database/).** 

Time Series data type
---------------------
This example stores data from sensors at a specific time in a type called `SensorValues`.
This type is defined in the file [ts-data-model.fbs](ts-data-model.fbs) like this:

```
table SensorValues {
    id: ulong;

    /// objectbox:id-companion,date
    time: long;

    temperatureOutside: double;
    temperatureInside: double;
    // ...
}
```

Please pay special attention to the `time` property and that it is annotated by `/// objectbox:id-companion,date`.
This specific annotation makes `SensorValues` a TS type and thus enables time series functionality for it.

And by the way, the mentioned .fbs file is a FlatBuffers schema definition,
which is also used by [ObjectBox Generator](https://github.com/objectbox/objectbox-generator).
The latter generates the `SensorValues` C++ struct and binding code to enable `obx::Box<SensorValues>`.   
 
Storing Time Series data
------------------------
All (non-generated) source code is contained in the [main.cpp](src/main.cpp) file.
As you will see, storing time series data is no different from storing other object data.
Using a templated `obx::Box`, it's a single call to `put()`:

    std::vector<SensorValues> values = createSensorValueData(now, dataCount);
    boxSV.put(values);

Note that `createSensorValueData()` creates dummy sensor data, which is mostly unrelated to ObjectBox.
The single special thing here is that the `SensorValues.id` member is set to `OBX_ID_NEW` to mark it as an new object.

License
-------
    Copyright 2020 ObjectBox Ltd. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
