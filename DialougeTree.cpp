#include "DialogueTree.h"

#include <utility>

DialogueNode::DialogueNode(std::string nodeText)
	: text(std::move(nodeText))
{
}

DialogueNode* DialogueNode::addChoice(
	const std::string& choice,
	const std::string& response)
{
	choices.push_back(choice);

	children.push_back(
		std::make_unique<DialogueNode>(response)
	);

	return children.back().get();
}