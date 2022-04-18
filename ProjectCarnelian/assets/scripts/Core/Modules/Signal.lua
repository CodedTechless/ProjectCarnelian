
local Connection = require("Connection");

---@class Signal
local Signal = {}
Signal.__class = "Signal";
Signal.__index = Signal;

function Signal.new()
    local self = setmetatable({}, Signal);

    self._header = nil;

    return self;
end

function Signal:Connect(func)
    local NewConnection = Connection.new(self, func);

    if self._header then
        NewConnection._next = self._header;
        self._header = NewConnection;
    else
        self._header = NewConnection;
    end

    return NewConnection;
end

function Signal:Fire(...)
    local item = self._header;

    while item do
        if item._connected then
            coroutine.wrap(item._func)(...);
        end

        item = item._next;
    end
end

function Signal:DisconnectAll()
    self._header = nil;
end

return Signal;