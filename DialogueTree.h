#pragma once

#include <memory>
#include <string>
#include <vector>

struct DialogueNode
{
	std::string text;
	std::vector<std::string> choices;
	std::vector<std::unique_ptr<DialogueNode>> children;

	explicit DialogueNode(std::string nodeText);

	DialogueNode* addChoice(
		const std::string& choice,
		const std::string& response
	);
};