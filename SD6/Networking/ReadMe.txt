 _   _      _                      _    _             
| \ | |    | |                    | |  (_)            
|  \| | ___| |___      _____  _ __| | ___ _ __   __ _ 
| . ` |/ _ \ __\ \ /\ / / _ \| '__| |/ / | '_ \ / _` |
| |\  |  __/ |_ \ V  V / (_) | |  |   <| | | | | (_| |
|_| \_|\___|\__| \_/\_/ \___/|_|  |_|\_\_|_| |_|\__, |
                                                 __/ |
                                                |___/ 

by Jeremy Hicks


//-----------------------------------------------------------------------------------------------
How To Test v2.0
- Open the first instance and type the following line in the console:
  '0'
  (simply a shorthand for the net_session_create_connection lines from A3)
- Open the second instance and type the following line the console:
  '1'
  (simply a shorthand for the net_session_create_connection lines from A3)
- Use WASD to move circles around

Note: Input handling is very fragile and does not check appropriately for errors.

- net_sim_loss 1.0 will drop all packets
- net_sim_lag 150 will add an additional delay of 150 milliseconds (different from A3)

Known Issues
- debug display only displays information for the current instance and not all connections
- ping command does not work
- some debug display information does not change over time, potentially indicating a problem
  with my implementation of reliable traffic


//-----------------------------------------------------------------------------------------------
How To Test v1.0
- Open the first instance and type the following lines in the console:
  'net_session_create_connection 0 PLAYER0 <associated_ip>'
  'net_session_create_connection 1 PLAYER1 <associated_ip>'
- Open the second instance and type the following lines the console:
  'net_session_create_connection 1 PLAYER1 <associated_ip>'
  'net_session_create_connection 0 PLAYER0 <associated_ip>'
- Use WASD to move circles around

Note: Input handling is very fragile and does not check appropriately for errors.

- net_sim_loss 1.0 will drop all packets
- net_sim_lag 1.0 will add an additional delay of 1 second