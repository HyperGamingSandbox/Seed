
local RendererHost = require("DD\\Renderer\\Host")

local Control = require("Control")


--[[
local Edit = Control.makeWrap(RText)
Edit:addEvents(Control.elibs.innerPosition)


local ConsoleEdit = Control.makeWrap(RText)
ConsoleEdit:addEvents({
	onAdd = function(self, parent)
		local x, y, w, h = parent:get(#ROF_X, 4)
		local bx, by = self:get(#ROF_BX, 2)
		self:set(#ROF_X, x + bx, y + h - by, 10, 10)
		self:set(#ROF_SKIPHOVER, false)

		self:cevent(#ROF_ONPARENTCHANGEPOS, self)
	end,
	onParentChangePos = function(self, parent)
		local x, y, w, h = parent:get(#ROF_X, 4)
		local bx, by = self:get(#ROF_BX, 2)
		self:set(#ROF_X, x + bx, y + h - by)

		self:cevent(#ROF_ONPARENTCHANGEPOS, self)
	end,
	onLClick = function(self)
		dprint("ConsoleEdit:onLClick")
		-- setup global input object
		RendererHost.setupKeyInputObject(self)
	end,
	onProcessKey = function(self, key, alt)
		dprint("ConsoleEdit:processKey")
	end
})


local Edit = Control:extend()

function Item:initialize(x, y, w, h, caption, onClick, current)
	local c2 = 0x7c7c7c
	if current then c2 = 0xffffff end
	local wrap = self:add(InnerRBox3:new(x, y, w, h, 0x7c7c7c, 1, c2))
	wrap:set(#ROF_LCLICK, function()
		onClick(caption)
	end)
	self:add(InnerRText:new(3, 3, caption, _get(0), 0xffffff))
end


]]--

local Substrate = Control.makeWrap(RRect)
Substrate:addEvents(Control.elibs.innerBottomPosition)

Substrate:addEvents({
	onAdd = function(self, parent)
		local x, y, w, h = parent:get(#ROF_X, 4)
		local bx, by = self:get(#ROF_BX, 2)
		self:set(#ROF_X, x + bx, y + h - by, w - 10)
		self:set(#ROF_SKIPHOVER, false)

		self:cevent(#ROF_ONPARENTCHANGEPOS, self)
	end,
	onParentChangePos = function(self, parent)
		local x, y, w, h = parent:get(#ROF_X, 4)
		local bx, by = self:get(#ROF_BX, 2)
		self:set(#ROF_X, x + bx, y + h - by, w - 10)

		self:cevent(#ROF_ONPARENTCHANGEPOS, self)
	end
})

local Text = Control.makeWrap(RText)
Text:addEvents(Control.elibs.innerPosition)

local Edit = Control:extend()

function Edit:initialize(x, y, w)

	self.substrate = self:add(Substrate:new(x, y, 100, 15, 0x404040))
	local EditControl = self
	self.substrate:set(#ROF_LCLICK, function(self)
		dprint("Substrate:onLClick")
		-- setup global input object
		RendererHost.setupKeyInputObject(EditControl)
	end)
	self.substrate:add(Text:new(0, 0, "asd", _get(0), 0xffffff))

end

function Edit:processKey(key, alt)
	dprint("Edit:processKey")
end


return Edit