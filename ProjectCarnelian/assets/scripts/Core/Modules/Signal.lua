
---@class ScriptConnection
---@param _signal Signal
---@param _func function
---@param _next ScriptConnection
---@param _connected boolean
local Connection = {}
Connection.__class = "ScriptConnection";
Connection.__index = Connection;

function Connection.new(signal, func)
    local self = setmetatable({}, Connection);

    self._connected = true;
    self._signal = signal;
    self._func = func;
    self._next = nil;

    return self;
end

function Connection:Disconnect()
    assert(self._connected);
    self._connected = false;

    if self._signal._header == self then
        self._signal._header = self._next;
    else
        local prev = self._signal._header;
        
		while prev and prev._next ~= self do
			prev = prev._next;
		end

		if prev then
			prev._next = self._next;
		end
    end
end


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