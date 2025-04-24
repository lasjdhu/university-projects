# Limitations

1. Message Validation
  - Input validation for message parameters (Username, ChannelID, DisplayName, etc.) is not fully implemented
  - Character set restrictions are not enforced
  - Maximum length validations for message contents are not implemented

2. UDP Implementation
  - Only authentication (AUTH) works reliably
  - Other commands (JOIN, MSG, etc.) do not work at all, probably because of poor handling of REPLY from server
  - Message retransmission mechanism exists but hasn't been thoroughly tested
  - Timeout handling (250ms) and retry count (3) are implemented but not verified in various network conditions
  - Dynamic port allocation works strangely
  - Duplicate message detection is not implemented

3. TCP Implementation
  - Works more reliably than UDP
  - All basic commands are functional
  - Some edge cases in error handling might not be covered
