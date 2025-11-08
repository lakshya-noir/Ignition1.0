import React from 'react';
import { StatusBar } from 'expo-status-bar';
import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { Text } from 'react-native';
import { TelemetryProvider } from './src/context/TelemetryContext';
import DashboardScreen from './src/screens/DashboardScreen';
import MapScreen from './src/screens/MapScreen';
import DataLogsScreen from './src/screens/DataLogsScreen';
import RideSummaryScreen from './src/screens/RideSummaryScreen';

const Tab = createBottomTabNavigator();

export default function App() {
  return (
    <TelemetryProvider>
      <NavigationContainer>
        <StatusBar style="light" />
        <Tab.Navigator
          screenOptions={{
            headerStyle: {
              backgroundColor: '#1f2937',
            },
            headerTintColor: '#fff',
            tabBarStyle: {
              backgroundColor: '#1f2937',
              borderTopColor: '#374151',
            },
            tabBarActiveTintColor: '#10b981',
            tabBarInactiveTintColor: '#9ca3af',
          }}
        >
          <Tab.Screen 
            name="Dashboard" 
            component={DashboardScreen}
            options={{
              tabBarIcon: () => <Text style={{ fontSize: 20 }}>📊</Text>,
            }}
          />
          <Tab.Screen 
            name="Map" 
            component={MapScreen}
            options={{
              tabBarIcon: () => <Text style={{ fontSize: 20 }}>🗺️</Text>,
            }}
          />
          <Tab.Screen 
            name="Logs" 
            component={DataLogsScreen}
            options={{
              tabBarIcon: () => <Text style={{ fontSize: 20 }}>📝</Text>,
            }}
          />
          <Tab.Screen 
            name="Summary" 
            component={RideSummaryScreen}
            options={{
              tabBarIcon: () => <Text style={{ fontSize: 20 }}>📈</Text>,
            }}
          />
        </Tab.Navigator>
      </NavigationContainer>
    </TelemetryProvider>
  );
}

